class Levels {
   static Levels {[
      {
         "background": {
            "color": [247, 226, 107, 255],
            "sprite": "aunt-bg",
         },
         "generateMode": "minefield",
         "sprite": "aunt",
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
         "endingText": ["you survived the conversation", "with cousin touglas!"],
         "nextlevel": ["levelending", 2],
      },
      {
         "background": {
            "color": [178, 220, 239, 255],
            "sprite": "uncle-recliner"
         },
         "generateMode": "rain",
         "sprite": "uncle",
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
         "spriteName": "uncle tony",
         "nextlevel": "ending",
      },
   ]}
}