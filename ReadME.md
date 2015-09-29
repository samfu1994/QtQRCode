qtQRrender Project

# please clone and test whether it runs corectly on your system

## sys env config:
* opencv lib should be recompiled using mingw under windows,
* SYS ENV VAR "OPENCV_HONE" should contain /lib, /include, /bin directory.
* SYS ENV VAR "PATH" should contain $(OPENCV_HOME)/bin

## NOTICE!
the origin function:

    int main(void);
has been changed to 

    int test_main(void);
    
    
## implementation details:
*   about halftone image data:
    
    the data