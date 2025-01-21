{
  lib,
  stdenv,
}:
stdenv.mkDerivation (finalAttrs: {
  pname = "libobliviscence";
  version = "0.0.1";

  src = ./.;
  enableParallelBuilding = true;
  installPhase = ''
    runHook preInstall

    mkdir -p $out/lib
    cp ${finalAttrs.pname}.so $out/lib/${finalAttrs.pname}.so

    runHook postInstall
  '';

  meta = {
    description = "Find for missing allocation checks";
    mainProgram = "obliviscence";
    maintainers = with lib.maintainers; [sigmanificient];
  };
})
