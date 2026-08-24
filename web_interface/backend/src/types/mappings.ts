export const mappings = {
    MicroscopeType: {
        "widefield": ["AxioScan 7", "AxioScan.Z1", "AxioZoom", "Lightsheet Z.1"],
        "confocal": ["LSM710 Imager", "LSM780 Observer", "LSM880 Examiner", "LSM880 Imager", "LSM880 Observer", "LSM980 NIR", "Marianas Spinning Disk"]
    },
    MicroscopeTypeFieldnames: ["ImageDocument.Metadata.Information.Instrument.Microscopes.Microscope.System.keyword"]
}