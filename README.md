# IoT-Enabled Hex Clock

This project enhances the original 3D-printed **Hex Clock** by [SHDesigns](https://www.thingiverse.com/SHDesigns), adding IoT functionality that allows you to control the clock's numbers via a Telegram chatbot. It combines the aesthetics of 3D printing with modern IoT technology to create a functional and interactive clock.

## Features
- **IoT Integration**: Control the clock's numbers remotely using a Telegram chatbot.
- **Customizable**: Modify the code to adjust features according to your needs.
- **Easy to Assemble**: Includes step-by-step instructions for assembling the clock and setting up the IoT functionality.

## Modifications
This project is a derivative of the **Hex Clock** by SHDesigns. The following modifications were made:
- Added IoT functionality using an ESP32-CAM microcontroller.
- Integrated a Telegram bot for remote control of the clock's numbers.
- Adjusted the original 3D model to accommodate additional electronic components.

## Attribution
This project is based on the original **Hex Clock** by [SHDesigns](https://www.thingiverse.com/SHDesigns), available on Thingiverse: [Hex Clock](https://www.thingiverse.com/thing:5242321).

## License
This project is licensed under the **Creative Commons Attribution-NonCommercial-ShareAlike (CC BY-NC-SA 4.0)** license. The license terms are as follows:
- **Attribution**: You must credit the original creator, SHDesigns.
- **NonCommercial**: You may not use this project for commercial purposes.
- **ShareAlike**: If you remix, transform, or build upon this project, you must distribute your contributions under the same license.

The full license text can be found [here](https://creativecommons.org/licenses/by-nc-sa/4.0/legalcode).

### Code License
The IoT code included in this project is licensed under the **MIT License**. This allows for greater flexibility in using and modifying the code independently of the 3D design files.

## How to Build the Project

### Materials Needed
- 3D printer and filament (for printing the Hex Clock design).
- ESP8266 microcontroller.
- Necessary electronic components (e.g., wires, resistors, power supply).
- Telegram account to set up the chatbot.

### Instructions
1. **3D Printing**:
   - Download and print the modified Hex Clock STL files included in this repository.
2. **Hardware Setup**:
   - Assemble the clock using the 3D-printed parts and electronic components.
   - Refer to the following image for the pins for the motor and ESP32:

     ![Motor and ESP32 Pins](images/Screenshot%202025-01-24%20213003.png)

3. **IoT Integration**:
   - Upload the provided IoT code to the ESP32-CAM microcontroller.
   - Set up the Telegram bot as follows:
     - Obtain your bot token from [BotFather](https://t.me/BotFather). Use the following code snippet in your program:
       ```cpp
       // Telegram Bot Token
       String BOTtoken = "";  // Your Bot Token
       // Create the bot here https://t.me/BotFather
       ```
       ![BotFather Screenshot](images/Screenshot%202025-01-24%20215802.png)

     - Authorize user chat IDs:
       ```cpp
       // Authorized User Chat IDs
       String authorizedChatIDs[] = {"0123456789"};
       // Get your ID here https://t.me/myidbot
       ```
       ![IDBot Screenshot](images/Screenshot%202025-01-24%20215832.png)

   - Follow the included instructions to finalize the Telegram bot setup.

4. **Testing**:
   - Test the Telegram chatbot to ensure it controls the clock's numbers as expected.

## Repository Contents
- `STL_Files/`: Contains the 3D design files for printing.
- `Code/`: Includes the Arduino sketch for the ESP8266 and Telegram bot.
- `Docs/`: Assembly instructions and wiring diagrams.
- `LICENSE`: The Creative Commons Attribution-NonCommercial-ShareAlike license.

## Contributing
Contributions to improve this project are welcome! If you'd like to contribute, please fork this repository and submit a pull request with your changes.

## Disclaimer
This project is for personal and educational use only. It cannot be sold or used for commercial purposes in compliance with the CC BY-NC-SA license.

---

If you have any questions or need help with setup, feel free to open an issue or contact me!
