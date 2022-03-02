# study

Installation 

1.copy files by
git clone https://github.com/grand-yarl/study.git

2.install package navigation_layers in your catkin_ws/src

3.install layer to your robot by adding to local_costmap_params.yaml 
  - {name: sonar_sensor_layer, type: "range_sensor_layer::RangeSensorLayer"}

4.build your workspace

Parameters in costmap_common_params.yaml:

enabled - Whether to apply this plugin or not, default = True

no_readings_timeout - No Readings Timeout , default = 0

clear_on_max_reading - Clear space in front of obtacle, default = True

footprint_clearing_enabled - Clear footprint from obstacles, default = False)

trust_time - Time of storing obstacle, default = 5.00



