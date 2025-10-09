# LibreWin
![GitHub License](https://img.shields.io/github/license/thurchito/LibreWin) ![GitHub Release Date](https://img.shields.io/github/release-date/thurchito/LibreWin) ![GitHub Downloads (all assets, all releases)](https://img.shields.io/github/downloads/thurchito/LibreWin/total) ![GitHub contributors](https://img.shields.io/github/contributors/thurchito/LibreWin) ![GitHub language count](https://img.shields.io/github/languages/count/thurchito/LibreWin) ![GitHub top language](https://img.shields.io/github/languages/top/thurchito/LibreWin)



**[LibreWin](https://thurchito.github.io/)** is an open source Windows compatible operating system.

>[!WARNING]
>This is free software and comes with ABSOLUTELY NO WARRANTY!

# Status
I'm currently working on porting all Windows 2000 system calls!
**⚠️ Info:** The current OS image implements stubs of NT API functions and therefore does NOT boot just yet. I'm working to resolve that.

# Minimum System Requirements
- i386 CPU
- 20MB of RAM
- 32MB of Storage

# Gallery
## 0.3.0 Beta 2
![image](https://github.com/user-attachments/assets/ca2b0490-efe6-49fa-bc39-7673d6d63337)
### Changelog
- added the Native Shell in graphical mode
- added a loader for ```.bat files```, and you can now run the provided executables in the shell
- added the ```print.h``` library to make it easier to use ```NtDisplayString``` in apps

# What it is
**[LibreWin](https://thurchito.github.io/)** combines the familiar Windows environment with the reliability and transparency of open-source software. It aims to provide full compatibility with Windows applications, ensuring your favorite games and programs run seamlessly.
Originally, LibreWin aimed to develop a custom-built kernel. However, we have now decided to fork the existing [Uinxed Kernel](https://github.com/ViudiraTech/Uinxed-Kernel) due to its stability and rich feature set. We are actively **integrating our NT Executive Layer** into this kernel, ensuring **seamless Windows application support**.

>[!NOTE]
> LibreWin is NOT a recreation or clone of the Microsoft Windows NT kernel. Instead, we enhance an existing kernel by implementing our own NT executive layer, allowing Windows applications to run smoothly.

# What it is NOT
- An NT Clone
- A Windows 95 Clone (NT 4.0! Not 95, althought NT 4 is the NT version of 95.)

# Project Purpose
Dependence on a large corporation for an operating system is profoundly disturbing. The goal of LibreWin is to eliminate Windows' bloatware and privacy issues while maintaining full software compatibility. With the adoption of the [Uinxed Kernel](https://github.com/ViudiraTech/Uinxed-Kernel), we can accelerate development while focusing on **refining the NT executive layer for optimal Windows application support**.

# How to join the project?
You can join this project by simply doing so, literally. Anyone is welcome to contribute! You can:
 - Submit a **pull request** with your contributions.
 - Join the development discussion on **[Discord](https://discord.com/)**: DM ```dripkap_19416``` or ```kappetrov```, if unable to do so, please open a github issue including your discord id so we can contact you.

# Running releases
To try LibreWin, you can either:
 1. **Compile from source** by running:

    ```
    make
    ```

    inside the ```librewin/``` directory.

 2. **Download a prebuilt binary** from the [Releases](https://github.com/thurchito/librewin/releases) section.
