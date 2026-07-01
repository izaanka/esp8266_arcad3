import re

with open('README.md', 'r') as f:
    content = f.read()

# Add categories to features
new_feature = "- **Categorized Folders**: 100 games are cleanly organized into 5 folders (`Arcade`, `Puzzle`, `Action`, `Sports`, `Other Games`) and sorted alphabetically for fast navigation.\n- **EEPROM Highscores**:"

content = content.replace("- **EEPROM Highscores**:", new_feature)

with open('README.md', 'w') as f:
    f.write(content)
