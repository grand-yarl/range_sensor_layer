class point_stamp
{
  private:
      unsigned int x;
      unsigned int y;
      double time;
      
  public:
      
      point_stamp(unsigned int x, unsigned int y)
      {
          this->x = x;
          this->y = y;
          this->time = ros::Time::now().toSec();
      }
      
      bool identify(unsigned int x, unsigned int y)
      {
          if ((x == this->x) && (y == this->y)) return true;
          else return false;
          
      }
      
      void update_time()
      {
           this->time = ros::Time::now().toSec();         
      }

      unsigned int get_x()
      {
          return this->x;
      }
      
      unsigned int get_y()
      {
          return this->y;
      }
      
      double get_time()
      {
          return this->time;
      }
};
