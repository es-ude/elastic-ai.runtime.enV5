module.exports = {
  extends : [ '@commitlint/config-conventional' ],
  rules : {
    'type-enum' :
                [
                  2, "always",
                  [
                    'build', 'ci', 'feat', 'fix', 'docs', 'style', 'refactor', 'revert', 'chore',
                    'wip', 'perf'
                  ]
                ],
    'scope-case' : [ 2, "always", "kebab-case" ],
    'scope-enum' :
                 [
                   2, "always",
                   [
                     'usb-protocol', 'hal', 'sensor', 'hardware-test', 'unit-test', 'middleware',
                     'stub', 'rtos', 'flash', 'all', 'utils', 'network', 'fpga', 'workflow'
                   ]
                 ],
    'subject-empty' : [ 2, "never" ],
  }
}
