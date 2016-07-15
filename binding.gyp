{
  "targets": [
    {
      "target_name": "bindings",
      'dependencies': [
        'librdkafka'
      ],
      "sources": [ "<!@(ls -1 src/*.cc)", ],
      "include_dirs": [
            "<!(node -e \"require('nan')\")",
            "deps/librdkafka/src-cpp"
      ],
      'conditions': [
        [ 'OS=="mac"', {
            'xcode_settings': {
              'MACOSX_DEPLOYMENT_TARGET': '10.11',
              'OTHER_CFLAGS' : ['-Wall', '-Wno-sign-compare', '-Wno-missing-field-initializers'],
            },
            'libraries' : ['-lz -lrdkafka -lrdkafka++']
          }
        ],
        [
          'OS=="linux"',
          {
            'cflags': ['-Wall', '-Wno-sign-compare', '-Wno-missing-field-initializers'],
            'libraries' : ['-lz -lrdkafka -lrdkafka++']
          }
        ]
      ]
    },
    # Configure librdkafka
    {
      "target_name": "librdkafka_config_h",
      "type": "none",
      "actions": [
        {
          'action_name': 'configure_librdkafka',
          'message': 'configuring librdkafka...',
          'inputs': [
            'deps/librdkafka/configure',
          ],
          'outputs': [
            'deps/librdkafka/config.h',
          ],
          'action': ['eval', 'cd deps/librdkafka && ./configure'],
        },
      ],
    },
    # Build librdkafka
    {
      "target_name": "librdkafka",
      "type": "none",
      'dependencies': [
        'librdkafka_config_h',
      ],
      "actions": [
        {
          'action_name': 'make_librdkafka',
          'message': 'building librdkafka...',
          'inputs': [
            'deps/librdkafka/Makefile',
          ],
          'outputs': [
            'deps/librdkafka/src/librdkafka.so',
            'deps/librdkafka/src-cpp/librdkafka++.so',
          ],
          'action': ['eval', 'cd deps/librdkafka && make'],
        },
      ],
    }
  ]
}