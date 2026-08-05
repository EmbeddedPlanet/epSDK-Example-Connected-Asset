[![epSDK Connected Asset Example][ep-logo]][ep-link]

[ep-logo]: logo.png
[ep-link]: https://www.embeddedplanet.com/
[ep-doc-link]: https://www.embeddedplanet.com/product-documentation/#connected-asset

# epSDK-Example-Connected-Asset
 
The epSDK for FreeRTOS provides static library files for sensors and communication interfaces, significantly reducing development time and testing costs. The epSDK provides integration to a user’s application through simple user-friendly API calls.

The intended use of epSDK is with EP supplied hardware and a FreeRTOS OS running on a Nordic nRF52840 microcontroller. Users can configure the communication interfaces and drivers through compiler defines that get passed to the libraries. A slightly modified nRF SDK is provided to users and includes the FreeRTOS kernel. The Sensors, Communication Interfaces, and Bootloader are contained within the epSDK. Users shall develop their individual applications but are provided with an example to get started. Individual drivers and communication interfaces are portable to other hardware using FreeRTOS on an nRF52840 microcontroller. Contact EP to discuss.

The epSDK for Connected Asset is compatible with the EP Galaxis.

Users will receive a slightly modified NRF SDK that must be located on the same folder level as this project, otherwise paths will need adjustment in the makefile.

•	Sales: sales@embeddedplanet.com

•	Information Requests: info@embeddedplanet.com

•	Technical Support: support@embeddedplanet.com

## License

The software is provided under the [Apache-2.0 license](LICENSE).

## Documentation
The epSDK User Guide and Connected Asset User Guide can be found on the [EP Documents Page][ep-doc-link]

## Getting Started
Set up the environment as described in the epSDK User Guide.

## Embedded Planet Documentation Control System 
Archived under P37300000000096
