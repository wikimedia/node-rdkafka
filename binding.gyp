{
  "targets": [
    {
      "target_name": "bindings",
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
        ]
      ]
    }
  ]
}