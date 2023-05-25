{
  lib,
  glib,
  makeWrapper,
  rustPlatform,
  atk,
  gtk3,
  gtk-layer-shell,
  pkg-config,
  cargo,
  rustc,
}: let
  cargoToml = builtins.fromTOML (builtins.readFile ./Cargo.toml);
in
  rustPlatform.buildRustPackage {
    src = ./.;
    buildInputs = [
      pkg-config
      glib
      atk
      gtk3
      gtk-layer-shell
    ];
    cargoLock = {
      lockFile = ./Cargo.lock;
    };
    checkInputs = [cargo rustc];
    nativeBuildInputs = [
      pkg-config
      makeWrapper
      rustc
      cargo
    ];

    CARGO_BUILD_INCREMENTAL = "false";
    RUST_BACKTRACE = "full";
    copyLibs = true;

    name = cargoToml.package.name;
    version = cargoToml.package.version;

    meta = with lib; {
      description = "A crippled lock";
      homepage = "";
      license = licenses.gpl3;
    };
  }
