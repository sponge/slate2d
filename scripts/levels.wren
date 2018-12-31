class Levels {
   static Levels {[
      {
         "background": {
            "color": [247, 226, 107, 255],
            "sprite": "aunt-bg",
         },
         "generateMode": "minefield",
         "sprite": "aunt",
         "spriteName": "aunt deborah",
         "endingText": ["you survived the conversation", "with aunt deborah!"],
         "nextlevel": ["levelending", 1],
         "help": true
      },
      {
         "background": {
            "color": [224, 111, 139, 255],
            "sprite": "cousin-bg"
         },
         "generateMode": "snow",
         "sprite": "cousin",
         "spriteName": "cousin touglas",
         "endingText": ["you survived the conversation", "with cousin touglas!"],
         "nextlevel": ["levelending", 2],
      },
      {
         "background": {
            "color": [178, 220, 239, 255],
            "sprite": "uncle-recliner"
         },
         "generateMode": "lob",
         "sprite": "uncle",
         "spriteName": "uncle tony",
         "endingText": ["you survived the conversation", "with uncle tony!"],
         "nextlevel": ["levelending", 3],
      },
      {
         "background": {
            "color": [218, 66, 0, 255],
            "sprite": "table",
            "spriteWidth": 72,
         },
         "generateMode": "ashes",
         "sprite": "table",
         "spriteName": "everyone",
         "nextlevel": "ending",
      },
   ]}

   static EndlessLevels { Levels.Levels + [
      {
         "background": {
            "color": [153, 100, 249, 255],
            "sprite": "uncle-recliner"
         },
         "generateMode": "lasers",
         "sprite": "uncle",
         "spriteName": "uncle tony",
         "endingText": ["you survived the conversation", "with uncle tony!"],
      },
      {
         "background": {
            "color": [0, 87, 132, 255],
            "sprite": "aunt-bg",
         },
         "generateMode": "rain",
         "sprite": "aunt",
         "spriteName": "aunt deborah",
         "endingText": ["you survived the conversation", "with aunt deborah!"],
      },
   ]}
}