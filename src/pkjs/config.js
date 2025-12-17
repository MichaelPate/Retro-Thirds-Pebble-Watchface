module.exports = [
      {
        "type": "heading",
        "defaultValue": "App Configuration"
      },
      {
        "type": "text",
        "defaultValue": "Change the settings for colors and the weather feature."
      },
      {
        "type": "section",
        "items": [
          {
            "type": "heading",
            "defaultValue": "Colors"
          },
          {
            "type": "color",
            "messageKey": "topBackgroundColor",
            "defaultValue": "0x5555FF",
            "label": "Top Third Background Color"
          },
          {
            "type": "color",
            "messageKey": "weatherTextColor",
            "defaultValue": "0x000000",
            "label": "Weather Text Color"
          },
          {
            "type": "color",
            "messageKey": "middleBackgroundColor",
            "defaultValue": "0xAAFFAA",
            "label": "Middle Third Background Color"
          },
          {
            "type": "color",
            "messageKey": "timeTextColor",
            "defaultValue": "0x000000",
            "label": "Time Text Color"
          },
          {
            "type": "color",
            "messageKey": "bottomBackgroundColor",
            "defaultValue": "0xAA5555",
            "label": "Bottom Third Background Color"
          },
          {
            "type": "color",
            "messageKey": "dateTextColor",
            "defaultValue": "0x000000",
            "label": "Date Text Color"
          }
        ]
      },
      {
        "type": "section",
        "items": [
          {
            "type": "heading",
            "defaultValue": "More Settings"
          },
          {
            "type": "text",
            "messageKey": "weathermapAPIkey",
            "label": "OWM API Key",
            "defaultValue": ""
          },
          {
            "type": "text",
            "messageKey": "weatherUpdateFrequencyMinutes",
            "label": "Update Frequency (minutes)",
            "defaultValue": "30"
          }
        ]
      },
      {
        "type": "submit",
        "defaultValue": "Save Settings"
      },
      {
        "type": "text",
        "defaultValue": "Made by Michael Pate, 12/2025"
      }
  ]
