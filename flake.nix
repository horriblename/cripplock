{
  description = "Crippled lock";

  outputs = {
    self,
    nixpkgs,
  }: let
    cargoToml = builtins.fromTOML (builtins.readFile ./Cargo.toml);
    supportedSystems = ["x86_64-linux" "aarch64-linux"];
    forAllSystems = f: nixpkgs.lib.genAttrs supportedSystems (system: f system);
    pkgsFor = system:
      import nixpkgs {
        inherit system;
        overlays = [self.overlay];
      };
  in {
    overlay = final: prev: {
      "${cargoToml.package.name}" = final.callPackage ./default.nix {};
    };

    packages = forAllSystems (system: let
      pkgs = pkgsFor system;
    in {
      "${cargoToml.package.name}" = pkgs."${cargoToml.package.name}";
      default = pkgs."${cargoToml.package.name}";
    });

    formatter = forAllSystems (system: (pkgsFor system).alejandra);

    devShell = forAllSystems (system: let
      pkgs = pkgsFor system;
    in
      pkgs.mkShell {
        inputsFrom = [pkgs.${cargoToml.package.name}];
        buildInputs = with pkgs; [
          rustc
          cargo
          rustfmt
          nixpkgs-fmt
        ];
      });
  };
}
