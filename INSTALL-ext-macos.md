# Install X Org Window system for OSX

https://xquartz.macosforge.org


# Install homebrew

```bash
ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```


# Register useful packages

```bash
brew tap homebrew/science
brew tap homebrew/x11
brew tap homebrew/python
brew tap cbenhagen/video
```


# With homebrew, you can get a full TuttleOFX installation

```bash
brew install tuttleofx
```

# To build TuttleOFX by yourself, you can use homebrew to install dependencies

```bash
# Print dependencies
brew deps tuttleofx

# Install dependencies
brew install tuttleofx --only-dependencies
```

