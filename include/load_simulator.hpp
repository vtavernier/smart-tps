#ifndef _LOAD_SIMULATOR_HPP_
#define _LOAD_SIMULATOR_HPP_

class LoadSimulatorImpl {
      public:
	virtual ~LoadSimulatorImpl() = default;

	virtual void begin() = 0;

	/**
	 * @brief Set the load status
	 *
	 * @param base true if the base should be on
	 * @param liner pulse width of the liner
	 * @param shader pulse width of the shader
	 */
	virtual void set_load(bool base, float liner, float shader) = 0;
};

extern LoadSimulatorImpl *new_load_simulator_impl();

class LoadSimulator {
	LoadSimulatorImpl *impl_;

      public:
	LoadSimulator();
	LoadSimulator(LoadSimulatorImpl *impl);
	~LoadSimulator();

	void begin();

	/**
	 * @brief Set the load status
	 *
	 * @param base true if the base should be on
	 * @param liner pulse width of the liner
	 * @param shader pulse width of the shader
	 */
	void set_load(bool base, float liner, float shader);
};

#endif /* _LOAD_SIMULATOR_HPP_ */
