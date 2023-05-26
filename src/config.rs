use std::os::unix::fs::{OpenOptionsExt, PermissionsExt};
use std::path::{Path, PathBuf};
use std::{env, fs, io};

const GROUP_PERMISSIONS: u32 = 0o070;
const OTHERS_PERMISSIONS: u32 = 0o007;

type RCResult<T> = Result<T, ReadConfigError>;
pub enum ReadConfigError {
    IOError(io::Error),

    /// Config files should only be accessible by the owner
    LaxPermissionError,

    /// The config file has invalid characters
    InvalidConfigError,
    // CreateFileError,
}

pub struct Config {
    pub pattern: Vec<u8>,
}

fn get_config_path() -> PathBuf {
    // let mut maybe_xdg_config_path = env::var("XDG_CONFIG_HOME").ok().and_then(|path_str| Some(PathBuf::from(path_str)));
    let mut xdg_config_home: PathBuf = env::var("XDG_CONFIG_HOME")
        .ok()
        .and_then(|path_str| Some(PathBuf::from(path_str)))
        .or_else(|| {
            let mut path = PathBuf::from(env::var("HOME").expect("$HOME is not set!"));
            path.push(".config");
            Some(path)
        })
        .expect("unreachable");
    xdg_config_home.push("cripplock");
    xdg_config_home
}

pub fn read_config() -> RCResult<Config> {
    let config_dir = get_config_path();
    let config_file = config_dir.join("pattern");

    match fs::metadata(&config_file) {
        Ok(metadata) => {
            let perm = metadata.permissions().mode();
            if perm & GROUP_PERMISSIONS != 0 || perm & OTHERS_PERMISSIONS != 0 {
                return Err(ReadConfigError::LaxPermissionError);
            }
            return read_config_file(&config_file);
        }
        Err(err) => Err(ReadConfigError::IOError(err)),
    }
}

fn read_config_file(config_file: &Path) -> RCResult<Config> {
    let pattern = fs::read(config_file).map_err(|err| ReadConfigError::IOError(err))?;
    let mut pattern_acc = Vec::new();

    for byte in pattern {
        if byte < b'1' || byte > b'9' {
            return Err(ReadConfigError::InvalidConfigError);
        }

        let n = byte - b'0';
        if pattern_acc.contains(&n) {
            return Err(ReadConfigError::InvalidConfigError);
        }

        pattern_acc.push(n);
    }

    Ok(Config {
        pattern: pattern_acc,
    })
}
