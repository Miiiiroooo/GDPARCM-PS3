Github: https://github.com/Miiiiroooo/GDPARCM-PS3.git

Installing gRPC:
- gRPC by itself is a huge library that cannot be included in the repo
- so here are some steps to install it
    # Cloning vcpkg
    1. Clone the repository
    2. Open the console on the "<repository>/P3_RealTimeSceneViewer" directory
    3. git config --global http.postBuffer 4194304000
    4. git config --global core.compression 0
    5. git clone --depth 1 --branch 2024.02.14 https://github.com/microsoft/vcpkg
    6. cd vcpkg
    7. git fetch --depth=<N> ; <with increasing N with each fetch>
        > e.g. depth=100 -> depth=500 -> depth=1000  ; if there errors, reduce the increments
    8. git fetch --unshallow  ; should return "fatal: --unshallow on a complete repository does not make sense"
        > if not, then continue fetching with depths 
    9. git pull
    # Visual Studio
    10. install Visual Studio 2022 with:
        a. Desktop development with C++
        b. Windows 11 SDK
        c. C++ cmake tools for Windows
    # Installing gRPC
    11. <back to the console window> .\vcpkg\bootstrap-vcpkg.bat
    12. vcpkg install protobuf protobuf:x64-windows 
    13. vcpkg install grpc:x64-windows 
    14. vcpkg integrate install


Steps to run the Projects:
    1. Open the Visual Studio Solution in the repository
    2. Right-click the solution and go to its Properties
    3. Inside "Startup Projects", make sure the following is on "Start:"
        a. P3_RealTimeSceneViewer
        b. P3_SceneLoader
    4. In the same window, make sure the other projects are in "None"
    5. Run the porject


Note: A build folder is also provided with the exe files for both the SceneViewer and SceneLoader
      It will ask for permission to the firewall; allow it or else the program(s) will not work


Controls in the Program:
- Mouse + LMB: camera rotation
- WASD: camera movement
- Space + LShift: up and down movement

Credits:
- All models are taken from this site: https://www.models-resource.com/pc_computer/leagueoflegends/ 