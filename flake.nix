{
  description = "A flake with all tools needed for this project.";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      allSystems = [
        "x86_64-linux" # 64-bit Intel/AMD Linux
        "aarch64-linux" # 64-bit ARM Linux
        "x86_64-darwin" # 64-bit Intel macOS
        "aarch64-darwin" # 64-bit ARM macOS
      ];

      forAllSystems = fn: 
        nixpkgs.lib.genAttrs allSystems
          (system: fn { pkgs = import nixpkgs { inherit system; }; });
    in
    {
      devShells = forAllSystems ({ pkgs }: {
        default = pkgs.mkShell {
          packages = (with pkgs; [
            git
            arduino-ide
            python3
            fritzing
          ]);
        };
      });


  };
}
