# ATLAS

Atlas is a multi-module application for float missions that combines robust backend processing with a responsive graphical user interface and an embedded client. The project is divided into three primary components:

- **NANO_CLOCK:** Provides high-precision timekeeping functionality.
- **float:** Contains the core application logic and algorithms implemented in C++. An embedded client (running on Arduino hardware) that handles sensor data, motor control, and communication via WiFi.
- **gui_float:** Implements the front-end interface using JavaScript, EJS, and CSS for a seamless user experience.

## Table of Contents

- [float\_mate\_2024](#float_mate_2024)
  - [Table of Contents](#table-of-contents)
  - [Overview](#overview)
  - [Application Details](#application-details)
    - [Core Logic (`float` Folder)](#core-logic-float-folder)
    - [Graphical User Interface (`gui_float` Folder)](#graphical-user-interface-gui_float-folder)
    - [High-Precision Timing (`NANO_CLOCK` Folder)](#high-precision-timing-nano_clock-folder)
    - [Arduino Client (Embedded Controller)](#arduino-client-embedded-controller)
  - [Installation](#installation)
    - [Prerequisites](#prerequisites)
    - [Setup Steps](#setup-steps)
  - [Usage](#usage)
  - [Project Structure](#project-structure)
  - [Contributing](#contributing)
  - [License](#license)
  - [Contact](#contact)

## Overview

The **float_mate_2024** project is designed to deliver a high-performance application where precise timekeeping, computational accuracy, and user-friendly interaction meet. It is ideal for scenarios where floating-point calculations, real-time display updates, and embedded control (via Arduino) are critical.

## Application Details

### Core Logic (`float` Folder)

The **float** folder houses the core backend functionality of the application. Key aspects include:

- **Floating-Point Operations:** Implements custom algorithms and data structures to handle high-precision floating-point computations. This may involve optimized mathematical functions and error-handling routines.
- **Modular Design:** The code is organized into several C++ modules to separate concerns, making it easier to maintain and extend. Functions are designed to be both efficient and reliable.
- **Integration:** Acts as the bridge between the high-precision clock (from the `NANO_CLOCK` module) and the user interface, processing data and delivering it in a format that the front-end can readily display.

*Note: Review the source files in the `float` folder for specific classes, methods, and algorithms implemented.*

### Graphical User Interface (`gui_float` Folder)

The **gui_float** folder is dedicated to the user-facing part of the application. Its highlights include:

- **Interactive Components:** Built using JavaScript and EJS templates, the GUI dynamically displays the results of the floating-point computations and timekeeping data. Users can interact with the interface to adjust settings, view results, or trigger specific operations.
- **Responsive Design:** CSS is used to ensure the interface adapts gracefully across different devices and screen sizes.
- **Real-Time Updates:** Integrates with the backend logic to provide real-time data visualization. The design leverages modern front-end development practices, ensuring that updates to the underlying data are reflected immediately in the GUI.

*Tip: Check the JavaScript and EJS files in the `gui_float` folder to understand how the front-end binds data and handles user interactions.*

### High-Precision Timing (`NANO_CLOCK` Folder)

The **NANO_CLOCK** folder offers high-precision timing functionality that:

- **Ensures Accurate Timing:** Provides a foundation for operations that depend on exact time measurements.
- **Synchronization:** Works in tandem with the core logic and GUI to trigger time-sensitive events and ensure that data is processed and displayed with minimal latency.

### Arduino Client (Embedded Controller)

The Arduino client code, found in the repository, is designed to run on an Arduino board and serves as the embedded controller. It manages sensor readings, motor operations, and network communication over WiFi. Key details include:

- **Libraries Used:**
  - **WiFi.h, WiFiClient.h, WebServer.h:** For establishing a WiFi connection and running a lightweight HTTP server on the Arduino.
  - **ArduinoJson.h:** For serializing and deserializing JSON data used to communicate with other components of the system.
  - **SPI.h:** For communicating with the pressure sensor over the SPI protocol.
  - **uRTCLib.h:** For interfacing with the Real Time Clock (RTC) module, which ensures accurate time measurements.
  - **math.h:** For performing essential mathematical computations.

- **Sensor and Motor Control:**
  - The code reads pressure sensor data using SPI communication and computes related values such as temperature and depth.
  - Motor control is handled via defined functions (`PushUp()`, `PushDown()`, and `Stopp()`), which control the state of the motor outputs (connected to pins `in3` and `in4`).
  - Calibration constants (e.g., `DepthZero`, `PressureZero`, `CalibrationConstant`) are used to adjust the readings for accurate results.

- **Network and Server Endpoints:**
  - **/getdata:** Returns a JSON document containing platform information, current time (from the RTC), pressure, and computed depth.
  - **/start:** Initiates a mission sequence that performs a series of motor actions—pushing up, stopping, then pushing down—and responds with a success message.
  - **/ping:** A heartbeat endpoint that confirms connectivity.
  - **/sendVal:** Accepts a JSON payload to update calibration parameters (`PressureZero`, `DepthZero`, and `MovingTime`).
  - **/deleteDate:** Clears the current JSON document data.
  - **/up and /down:** Provide manual controls for triggering upward or downward motor actions.

- **Time Management:**
  - The RTC is used to provide a human-readable timestamp in the format `HH:MM:SS UTC`. This ensures that every data payload sent includes an accurate timestamp.
  
- **Error Handling:**
  - The code includes error handling in JSON operations and network responses to ensure that any issues (e.g., failure to parse JSON) are gracefully reported back to the client.

*Review the Arduino source code for further details on the implementation of these functionalities and to adapt the code for your specific mission or calibration requirements.*

## Installation

### Prerequisites

Before setting up the project, ensure you have the following installed:

- **C++ Compiler:** (e.g., GCC, Clang, or MSVC) for building the core logic and clock modules.
- **Node.js and npm:** For managing JavaScript dependencies and running the front-end.
- **Arduino IDE or PlatformIO:** To compile and upload the Arduino client code.
- **Git:** To clone the repository.

### Setup Steps

1. **Clone the Repository:**

   ```bash
   git clone https://github.com/Ali-Hassan2005/float_mate_2024.git
   cd float_mate_2024
   ```

2. **Build the Core Components:**

   - **C++ Modules (NANO_CLOCK and float):**  
     Navigate to each directory and follow the provided build instructions (e.g., using `make` if a Makefile is available):

     ```bash
     cd NANO_CLOCK
     make
     cd ../float
     make
     ```

3. **Install Front-End Dependencies:**

   Navigate to the `gui_float` directory and install the required npm packages:

   ```bash
   cd ../gui_float
   npm install
   ```

4. **Set Up the Arduino Client:**

   - Open the Arduino IDE or PlatformIO.
   - Import the Arduino client source code.
   - Ensure all necessary libraries (e.g., WiFi, ArduinoJson, uRTCLib) are installed.
   - Configure your WiFi credentials in the source code if necessary.
   - Compile and upload the code to your Arduino board.

## Usage

After installation, the components can be executed individually or as part of an integrated system.

- **Running the Core Application:**
  
  Execute the compiled binaries from the `float` and `NANO_CLOCK` folders to initialize the core services.

- **Starting the GUI:**

  In the `gui_float` folder, start the development server:

  ```bash
  npm start
  ```

  The GUI will launch in your default browser, displaying real-time data from the backend.

- **Operating the Arduino Client:**
  
  Once the Arduino client is running:
  
  - It will connect to the specified WiFi network and start an HTTP server on port 80.
  - Use the provided endpoints (e.g., `/getdata`, `/start`, `/ping`, `/sendVal`, `/up`, `/down`) to interact with the Arduino client.
  - Monitor the serial output for real-time sensor data (pressure and depth) and status messages.

## Project Structure

```plaintext
float_mate_2024/
├── NANO_CLOCK/         # High-precision clock module (C++)
├── float/              # Core application logic and floating-point operations (C++)
├── gui_float/          # Front-end interface (JavaScript, EJS, CSS)
├── Arduino_Client/     # Arduino client code for sensor reading, motor control, and WiFi communication
├── .gitignore          # Git configuration file to ignore specified files/folders
└── README.md           # Project documentation
```

## Contributing

Contributions to **float_mate_2024** are welcome! To contribute:

1. Fork the repository.
2. Create a branch for your feature or bug fix.
3. Commit your changes with clear commit messages.
4. Open a pull request describing your improvements.

Feel free to open an issue if you have any questions or need guidance.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for further details.

## Contact

For inquiries or feedback, please reach out via [GitHub Issues](https://github.com/Ali-Hassan2005/float_mate_2024/issues) or contact the repository maintainer directly.

---
