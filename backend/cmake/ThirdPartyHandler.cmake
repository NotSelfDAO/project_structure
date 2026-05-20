function(handle_third_party target_name target_repo target_tag)
    FetchContent_Declare(
        ${target_name}
        GIT_REPOSITORY ${target_repo}
        GIT_TAG ${target_tag}
    )
    FetchContent_MakeAvailable(${target_name})
endfunction()