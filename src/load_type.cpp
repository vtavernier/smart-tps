#include "load_type.hpp"

LoadType alternate(LoadType type) {
	switch (type) {
	case LoadType::Liner:
		return LoadType::Shader;
	case LoadType::Shader:
		return LoadType::Liner;
	}

	// Keep the compiler happy
	return LoadType::Liner;
}
