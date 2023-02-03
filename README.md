# Weather Router
A weather router for sail boats.

## Features
- Launch a GTK application

## Resources
- https://docs.gtk.org/gtk4/getting_started.html

## TODO
- Be able to view charts
  - Plot a part of an image using opengl and glarea
    - [x] Create a glarea
    - [ ] Display an image in the opengl area
  - Figure out where to download charts:
    - https://www.naturalearthdata.com/ 1:10m physical vectors
      - Load shapefile using https://github.com/OSGeo/shapelib
    - https://www.gebco.net/data_and_products/gridded_bathymetry_data/ (probably not usefull)
    - https://visibleearth.nasa.gov/collection/1484/blue-marble?page=2 
  - Plot charts
  - Plot with correct coordinates on the chart
- Plot boats
  - Plot boat sprite at correct position and heading
- Show wind
  - Load wind data
    - Get data from (http://www.tecepe.com.br/wiki/index.php?title=NOAAWinds)
    - Decode using https://github.com/LouJo/grib2dec or other library
  - Show wind on map
- Model boat movement
  - With different wind speeds and angles
  - Choose optimal sail if more are available
  - Compensate for time loss because of manouvering
- Create a basic routing algorithm
  - Start with ACO or other stochastic algorithm


## Dependencies
- libgtk-4-dev
- libglew-dev