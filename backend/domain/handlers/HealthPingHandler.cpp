#include "../../core/api/BackendDev.h"

namespace
{
    /**
     * @brief reality modify this class into realy handler mainly, this is just a template for demo and testing purpose
     * 
     */
class WriteTemplateHandler final : public backend_core::v1::domain::CommandHandler
{
protected:
    std::optional<backend_core::v1::domain::DomainError> Validate(
        const backend_core::v1::TaskRequest& request) const override
    {
        if (request.taskId.empty())
        {
            return backend_core::v1::domain::DomainError{
                backend_core::v1::domain::DomainErrorCode::InvalidArgument,
                "task_id_required",
                false};
        }
        return std::nullopt;
    }

    backend_core::v1::TaskEnvelope ExecuteCommand(const backend_core::v1::TaskRequest& request) const override
    {
        return backend_core::v1::domain::BuildSuccessEnvelope(request, "write_template_ok");
    }
};

backend_core::v1::TaskEnvelope HandleWriteTemplateEntry(const backend_core::v1::TaskRequest& request)
{
    return backend_core::v1::domain::InvokeFrameworkHandler<WriteTemplateHandler>(request);
}

AUTO_REGISTER("template", "write", "upsert", HandleWriteTemplateEntry);
}
