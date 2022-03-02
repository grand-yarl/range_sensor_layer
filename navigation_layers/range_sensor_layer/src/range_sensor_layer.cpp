// Copyright 2018 David V. Lu!!
#include <range_sensor_layer/range_sensor_layer.h>
#include <boost/algorithm/string.hpp>
#include <geometry_msgs/PointStamped.h>
#include <pluginlib/class_list_macros.h>
#include <angles/angles.h>
#include <algorithm>
#include <list>
#include <limits>
#include <map>
#include <string>
#include <utility>

PLUGINLIB_EXPORT_CLASS(range_sensor_layer::RangeSensorLayer, costmap_2d::Layer)

using costmap_2d::FREE_SPACE;
using costmap_2d::LETHAL_OBSTACLE;
using costmap_2d::NO_INFORMATION;

namespace range_sensor_layer
{

RangeSensorLayer::RangeSensorLayer() {}

void RangeSensorLayer::onInitialize()
{
  ros::NodeHandle nh("~/" + name_);
  current_ = true;
  buffered_readings_ = 0;
  last_reading_time_ = ros::Time::now();
  default_value_ = to_cost(0.5);

  matchSize();
  resetRange();

  // Default topic names list contains a single topic: /sonar
  // We use the XmlRpcValue constructor that takes a XML string and reading start offset
  const char* xml = "<value><array><data><value>/sonar</value></data></array></value>";
  int zero_offset = 0;
  std::string topics_ns;
  XmlRpc::XmlRpcValue topic_names(xml, &zero_offset);

  nh.param("ns", topics_ns, std::string());
  nh.param("topics", topic_names, topic_names);

  InputSensorType input_sensor_type = ALL;
  std::string sensor_type_name;
  nh.param("input_sensor_type", sensor_type_name, std::string("ALL"));

  nh.param("use_decay", use_decay_, true);
  nh.param("pixel_decay", pixel_decay_, 100.0);
  nh.param("transform_tolerance_", transform_tolerance_, 0.3);

  boost::to_upper(sensor_type_name);
  ROS_INFO("%s: %s as input_sensor_type given", name_.c_str(), sensor_type_name.c_str());

  if (sensor_type_name == "VARIABLE")
    input_sensor_type = VARIABLE;
  else if (sensor_type_name == "FIXED")
    input_sensor_type = FIXED;
  else if (sensor_type_name == "ALL")
    input_sensor_type = ALL;
  else
  {
    ROS_ERROR("%s: Invalid input sensor type: %s", name_.c_str(), sensor_type_name.c_str());
  }

  // Validate topic names list: it must be a (normally non-empty) list of strings
  if ((topic_names.valid() == false) || (topic_names.getType() != XmlRpc::XmlRpcValue::TypeArray))
  {
    ROS_ERROR("Invalid topic names list: it must be a non-empty list of strings");
    return;
  }

  if (topic_names.size() < 1)
  {
    // This could be an error, but I keep it as it can be useful for debug
    ROS_WARN("Empty topic names list: range sensor layer will have no effect on costmap");
  }

  // Traverse the topic names list subscribing to all of them with the same callback method
  for (int i = 0; i < topic_names.size(); i++)
  {
    if (topic_names[i].getType() != XmlRpc::XmlRpcValue::TypeString)
    {
      ROS_WARN("Invalid topic names list: element %d is not a string, so it will be ignored", i);
    }
    else
    {
      std::string topic_name(topics_ns);
      if ((topic_name.size() > 0) && (topic_name.at(topic_name.size() - 1) != '/'))
        topic_name += "/";
      topic_name += static_cast<std::string>(topic_names[i]);

      if (input_sensor_type == VARIABLE)
        processRangeMessageFunc_ = boost::bind(&RangeSensorLayer::processVariableRangeMsg, this, _1);
      else if (input_sensor_type == FIXED)
        processRangeMessageFunc_ = boost::bind(&RangeSensorLayer::processFixedRangeMsg, this, _1);
      else if (input_sensor_type == ALL)
        processRangeMessageFunc_ = boost::bind(&RangeSensorLayer::processRangeMsg, this, _1);
      else
      {
        ROS_ERROR(
          "%s: Invalid input sensor type: %s. Did you make a new type and forgot to choose the subscriber for it?",
          name_.c_str(), sensor_type_name.c_str());
      }

      range_subs_.push_back(nh.subscribe(topic_name, 100, &RangeSensorLayer::bufferIncomingRangeMsg, this));

      ROS_INFO("RangeSensorLayer: subscribed to topic %s", range_subs_.back().getTopic().c_str());
    }
  }

  dsrv_ = new dynamic_reconfigure::Server<range_sensor_layer::RangeSensorLayerConfig>(nh);
  dynamic_reconfigure::Server<range_sensor_layer::RangeSensorLayerConfig>::CallbackType cb =
    boost::bind(&RangeSensorLayer::reconfigureCB, this, _1, _2);
  dsrv_->setCallback(cb);
  global_frame_ = layered_costmap_->getGlobalFrameID();
}

void RangeSensorLayer::reconfigureCB(range_sensor_layer::RangeSensorLayerConfig &config, uint32_t level)
{
  phi_v_ = config.phi;
  no_readings_timeout_ = config.no_readings_timeout;
  clear_on_max_reading_ = config.clear_on_max_reading;
  footprint_clearing_enabled_ = config.footprint_clearing_enabled;
  trust_time_ = config.trust_time;

  if (enabled_ != config.enabled)
  {
    enabled_ = config.enabled;
    current_ = false;
  }
}

void RangeSensorLayer::bufferIncomingRangeMsg(const sensor_msgs::RangeConstPtr& range_message)
{
  boost::mutex::scoped_lock lock(range_message_mutex_);
  range_msgs_buffer_.push_back(*range_message);
}

void RangeSensorLayer::updateCostmap()
{
  std::list<sensor_msgs::Range> range_msgs_buffer_copy;

  range_message_mutex_.lock();
  range_msgs_buffer_copy = std::list<sensor_msgs::Range>(range_msgs_buffer_);
  range_msgs_buffer_.clear();
  range_message_mutex_.unlock();

  for (std::list<sensor_msgs::Range>::iterator range_msgs_it = range_msgs_buffer_copy.begin();
       range_msgs_it != range_msgs_buffer_copy.end(); range_msgs_it++)
  {
    processRangeMessageFunc_(*range_msgs_it);
  }
}

void RangeSensorLayer::processRangeMsg(sensor_msgs::Range& range_message)
{
  if (range_message.min_range == range_message.max_range)
    processFixedRangeMsg(range_message);
  else
    processVariableRangeMsg(range_message);
}

void RangeSensorLayer::processFixedRangeMsg(sensor_msgs::Range& range_message)
{
  if (!std::isinf(range_message.range))
  {
    ROS_ERROR_THROTTLE(1.0,
                       "Fixed distance ranger (min_range == max_range) in frame %s sent invalid value. "
                       "Only -Inf (== object detected) and Inf (== no object detected) are valid.",
                       range_message.header.frame_id.c_str());
    return;
  }


  if (range_message.range > 0)  // +inf
  {
    if (!clear_on_max_reading_)
      return;  // no clearing at all

  }

  range_message.range = range_message.min_range;

  updateCostmap(range_message);
}

void RangeSensorLayer::processVariableRangeMsg(sensor_msgs::Range& range_message)
{
  if (range_message.range < range_message.min_range || range_message.range > range_message.max_range)
    return;


  updateCostmap(range_message);
}

void RangeSensorLayer::updateCostmap(sensor_msgs::Range& range_message)
{
  max_angle_ = range_message.field_of_view / 2;

  geometry_msgs::PointStamped in, out;
  in.header.stamp = range_message.header.stamp;
  in.header.frame_id = range_message.header.frame_id;

  if (!tf_->canTransform(global_frame_, in.header.frame_id, in.header.stamp, ros::Duration(transform_tolerance_)))
  {
    ROS_ERROR_THROTTLE(1.0, "Range sensor layer can't transform from %s to %s at %f",
                       global_frame_.c_str(), in.header.frame_id.c_str(),
                       in.header.stamp.toSec());
    return;
  }

  tf_->transform(in, out, global_frame_);

  double ox = out.point.x, oy = out.point.y;

  in.point.x = range_message.range;

  tf_->transform(in, out, global_frame_);

  double tx = out.point.x, ty = out.point.y;

  // calculate target props
  double dx = tx - ox, dy = ty - oy, d = sqrt(dx * dx + dy * dy), theta = atan2(dy, dx);
  
        
  // Update Map with Target Point
  unsigned int aa, ab, ba, bb;
  bool point_exist = false;
  if (worldToMap(tx, ty, aa, ab))
  {
    for (double angle_incr = theta - range_message.field_of_view / 2; angle_incr <= theta + range_message.field_of_view / 2; angle_incr += 0.05)
    {
        if (d < range_message.max_range - 0.1)  //Marking a obstacle points 
        {
            tx = ox + d * cos(angle_incr);
            ty = oy + d * sin(angle_incr);
            worldToMap(tx, ty, aa, ab);
            setCost(aa, ab, LETHAL_OBSTACLE);
            for (long unsigned int i = 0; i < points.size(); i++)  
            {
                if (points[i].identify(aa, ab) == true)  //updating information for obstacles
                {
                    points[i].update_time();
                    point_exist = true;
                    break;
                }
            }
            if (point_exist == false)  //creating new obstacle point
            {
                point_stamp new_point(aa, ab);
                points.push_back(new_point);
            }  
        }   
        
        
        double r_incr = 0;
        double mx = ox;
        double my = oy;
        worldToMap(mx, my, ba, bb);
        int index = getIndex(ba, bb);
        while (aa != ba && ab != bb && r_incr < d && clear_on_max_reading_)  //Clearing space in front of obstacle 
        {
            if (costmap_[index] == LETHAL_OBSTACLE)
            {
                for (long unsigned int i = 0; i < points.size(); i++)  //Clearing memory of uncorrect obtacles
                {
                    if (points[i].identify(aa, ab) == true) 
                    {
                        points.erase(points.begin() + i);
                    }
                }
            }
            setCost(ba, bb, FREE_SPACE);
            r_incr += 0.1;
            mx = ox + r_incr * cos(angle_incr);
            my = oy + r_incr * sin(angle_incr);
            worldToMap(mx, my, ba, bb);
            index = getIndex(ba, bb);
        }
    }
  }
  
  buffered_readings_++;
  last_reading_time_ = ros::Time::now();
}

void RangeSensorLayer::resetRange()
{
  min_x_ = min_y_ =  std::numeric_limits<double>::max();
  max_x_ = max_y_ = -std::numeric_limits<double>::max();
}

void RangeSensorLayer::updateBounds(double robot_x, double robot_y, double robot_yaw, double* min_x, double* min_y, double* max_x, double* max_y)
{
  if (layered_costmap_->isRolling())
    updateOrigin(robot_x - getSizeInMetersX() / 2, robot_y - getSizeInMetersY() / 2);

  for (long unsigned int i = 0; i < points.size(); i++)
  {
      if (ros::Time::now().toSec() - points[i].get_time() > trust_time_)  //clearing outdated points
      {
          unsigned int mx = points[i].get_x();
          unsigned int my = points[i].get_y();
          points.erase(points.begin() + i);
          setCost(mx, my, FREE_SPACE);    
      }      
  }
  
  
  
  updateCostmap();

  *min_x = std::min(*min_x, min_x_);
  *min_y = std::min(*min_y, min_y_);
  *max_x = std::max(*max_x, max_x_);
  *max_y = std::max(*max_y, max_y_);
  
  resetRange();
  
  if (buffered_readings_ == 0)
  {
    if (no_readings_timeout_ > 0.0 &&
        (ros::Time::now() - last_reading_time_).toSec() > no_readings_timeout_)
    {
      ROS_WARN_THROTTLE(2.0, "No range readings received for %.2f seconds, " \
                        "while expected at least every %.2f seconds.",
                        (ros::Time::now() - last_reading_time_).toSec(), no_readings_timeout_);
      current_ = false;
    }
  }

  if (!enabled_)
  {
    current_ = true;
    return;
  }
  updateFootprint(robot_x, robot_y, robot_yaw, min_x, min_y, max_x, max_y);
}

void RangeSensorLayer::updateFootprint(double robot_x, double robot_y, double robot_yaw, double* min_x, double* min_y, double* max_x, double* max_y)
{
    if (!footprint_clearing_enabled_) return;
    costmap_2d::transformFootprint(robot_x, robot_y, robot_yaw, getFootprint(), transformed_footprint_);

    for (unsigned int i = 0; i < transformed_footprint_.size(); i++)
    {
      touch(transformed_footprint_[i].x, transformed_footprint_[i].y, min_x, min_y, max_x, max_y);
    }
}


void RangeSensorLayer::updateCosts(costmap_2d::Costmap2D& master_grid, int min_i, int min_j, int max_i, int max_j)
{
  if (!enabled_)
    return;
  
  if (footprint_clearing_enabled_)
  {
      setConvexPolygonCost(transformed_footprint_, costmap_2d::FREE_SPACE);
  }
  
  for (int j = min_j; j < max_j; j++)
  {
    for (int i = min_i; i < max_i; i++)
    {
      int index = getIndex(i, j);
      
      
      bool point_exist = false;
      if (costmap_[index] == LETHAL_OBSTACLE)  //Cheaking for uncorrect information 
      {
          for (long unsigned int m = 0; m < points.size(); m++)
          {
              if (points[m].identify(i, j) == true)
              {
                  point_exist = true;
                  break;
              }
          }
          if (point_exist == false)
          {
              costmap_[index] = NO_INFORMATION;
          } 
      }
              
      
      if (costmap_[index] == NO_INFORMATION)
        continue;
      master_grid.setCost(i, j, costmap_[index]); 
    }
  }
  
  buffered_readings_ = 0;
  current_ = true;
}

void RangeSensorLayer::reset()
{
  ROS_DEBUG("Reseting range sensor layer...");
  deactivate();
  resetMaps();
  current_ = true;
  activate();
}

void RangeSensorLayer::deactivate()
{
  range_msgs_buffer_.clear();
}

void RangeSensorLayer::activate()
{
  range_msgs_buffer_.clear();
}

}  // namespace range_sensor_layer
