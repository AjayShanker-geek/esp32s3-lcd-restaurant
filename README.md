# LVGL Demos Example

An example used to run LVGL's demos on ESP32-S3-LCD-EV-Board. Internal demos include music player, widgets, stress and benchmark. What's more, the printer and tuner are generated from [Squareline](https://squareline.io/).

This example alse shows two methods to avoid tearing effect. It uses two frame buffers and semaphores based on LVGL **buffering modes**. For more information about this, please refer to official [documents](https://docs.lvgl.io/master/porting/display.html?#buffering-modes). The implementation principles of them are shown as follows:
