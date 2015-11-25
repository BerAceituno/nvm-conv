# NVM - Converter
NVM model format converter software developed for the U.S.B. Mechatronics Group.

# Requirements

This software requires the following packages:

- CMake 2.6 or higher

And has been succesfully tested on Ubuntu 14.04 and 12.04, no GPU required.

# Installation

* **Download dependencies**

  To install CMake and Build-essentials enter your workspace and enter the following:
  
  ```
    $ sudo apt-get install build-essential
    $ sudo apt-get install cmake
  ```

* **Cloning the repository and building**

  locate the terminal in your workspace folder and enter:
  
  ```
    $ git clone https://github.com/BerAceituno/nvm-conv.git
  ```
  
  locate the terminal in the source code folder and build by entering the following commands:
  
  ```
    $ cd nvm-conv
    $ cmake . 
    $ make
  ```

# Use

The software is ran via terminal by locating in the build folder and entering:

```
  $ ./NVM_CONV -i  <input model .nvm> -o <output .PLY model> -cm <output .CSV model> -cc <output .CSV camera poses> -t <.CSV uses tabs instead of commas>
```

where the inputs represent:
```
  1. -i    :   an .NVM pointcloud with camera poses.
  
  2. -o    :   Output .PLY file with the pointcloud (Note the output directory must be already created).
  
  4. -cm   :   (optional) Output .CSV file with the model point cloud (includes <XYZ> <RGB>).
  
  5. -cc   :   (optional) Output .CSV file with the camera poses in the point cloud for visual odometry (includes <focal length> <Centroid WXYZ> <XYZ> <radial distortion>). 

  6. -t    :   (optional) if given the separation for the .CSV files will be a tab.
```

**Distributed under an MIT License**
