module.exports = {
  extends : [ '@commitlint/config-conventional' ],
  rules : {
    'type-enum' :
    [
      2, "always",
      [
        'build', 'ci', 'feat', 'fix', 'docs', 'style', 'refactor', 'revert', 'chore', 'wip', 'perf'
      ]
    ]
  }
}
