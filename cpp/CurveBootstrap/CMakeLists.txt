add_executable(CurveBootstrap CurveBootstrap.cpp)
target_link_libraries(CurveBootstrap PRIVATE QuantLib::QuantLib)

install(TARGETS CurveBootstrap
        RUNTIME DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
        )