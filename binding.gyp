{
  'variables': {
      # may be redefined in command line on configuration stage
      'BUILD_LIBRDKAFKA%': 0,
  },
  "targets": [
    {
      "target_name": "bindings",
      "sources": [ "<!@(ls -1 src/*.cc)", ],
      "include_dirs": [
        "<!(node -e \"require('nan')\")",
      ],
      'libraries' : ['-lz'],
      'conditions': [
        # If BUILD_LIBRDKAFKA, then
        # depend on librdkafka target, and use src-cpp when building
        # this target.
        [ '<(BUILD_LIBRDKAFKA)==1',
          {
            'dependencies': ['librdkafka'],
            'include_dirs': ["deps/librdkafka/src-cpp"],
          },
          # Else link against globally installed rdkafka and use
          # globally installed headers.  On Debian, you should
          # install the librdkafka1, librdkafka++1, and librdkafka-dev
          # .deb packages.
          {
            'libraries': ['-lz', '-lrdkafka', '-lrdkafka++'],
            'include_dirs': [
                "/usr/include/librdkafka",
                "/usr/local/include/librdkafka"
            ],
          },
        ],
        [ 'OS=="mac"',
          {
            'xcode_settings': {
              'MACOSX_DEPLOYMENT_TARGET': '10.11',
              'OTHER_CFLAGS' : ['-Wall', '-Wno-sign-compare', '-Wno-missing-field-initializers'],
            },
          }
        ],
        [ 'OS=="linux"',
          {
            'cflags': ['-Wall', '-Wno-sign-compare', '-Wno-missing-field-initializers'],
          }
        ]
      ]
    },

    # Build deps/librdkafka
    {
      "target_name": "librdkafka",
      "type": "none",

      'conditions': [
        # Vary outputs of the build action based on OS.
        [ 'OS=="mac"',
          {
            'variables' : {
              'OUTPUTS': [
                'deps/librdkafka/src/librdkafka.1.dylib',
                'deps/librdkafka/src-cpp/librdkafka++.dylib'
              ],
            },
          },
        ],
        [ 'OS=="linux"',
          {
            'variables' : {
              'OUTPUTS': [
                'deps/librdkafka/src/librdkafka.so',
                'deps/librdkafka/src-cpp/librdkafka++.so',
              ],
            }
          }
        ]
      ],

      "actions": [
        {
          # TODO: should this be two actions, conifgure && make?
          'action_name': 'build_librdkafka',
          'inputs': [
            'deps/librdkafka/configure',
          ],
          'outputs': [ '<@(OUTPUTS)' ],
          'conditions': [
            # If BUILD_LIBRDKAFKA, then configure and make deps/librdkafka
            [ '<(BUILD_LIBRDKAFKA)==1',
              {
                'message': 'Building librdkafka...',
                'action': ['eval', 'cd deps/librdkafka && ./configure && make'],
              },
              # Else do nothing. TODO: this feels a little hacky. But it works!
              {
                'message': 'NOT building local librdkafka, using globally installed binaries and headers.',
                "action": [],
              }
            ],
          ]

        },
      ],
    }
  ]
}
