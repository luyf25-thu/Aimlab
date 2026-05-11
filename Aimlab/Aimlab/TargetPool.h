#pragma once

#include <vector>
#include <memory>
#include "StaticTarget.h"

// 标靶对象池：复用靶子实例
class TargetPool
{
public:
    // 构造时预分配对象
    explicit TargetPool(std::size_t size = 50)
    {
        pool.reserve(size);
        for (std::size_t i = 0; i < size; ++i)
        {
            pool.emplace_back(std::make_unique<StaticTarget>());
        }
    }

    // 获取一个空闲靶子
    Target* acquireTarget()
    {
        for (const auto& target : pool)
        {
            if (!target->getIsActive())
            {
                return target.get();
            }
        }
        return nullptr;
    }

    // 释放靶子到对象池
    void releaseTarget(Target* target)
    {
        if (target)
        {
            target->deactivate();
        }
    }

    // 获取靶子池用于遍历
    const std::vector<std::unique_ptr<Target>>& getTargets() const
    {
        return pool;
    }

private:
    std::vector<std::unique_ptr<Target>> pool;
};
