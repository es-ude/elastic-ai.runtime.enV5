# Contribution Guidelines

## Naming Scheme

|        Scope | Scheme               |                          |
| -----------: |:---------------------|:-------------------------|
|        Files | PascalScheme         |                          |
|    Functions | camelCase            |                          |
|    Variables | camelCase            | suffix: typedef => `_t`  |
| Preprocessor | SCREAMING_SNAKE_CASE |                          |

Functions and global variables should be prefixed with a consistent identifier (e.g. the file name where the function
prototype resides).

## Publish Modifications

Don't push directly to the `main` branch. Push your modification to a new branch and open a pull request to `main`, so
that the maintainer of this repository can merge your modifications.
