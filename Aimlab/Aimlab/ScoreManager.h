#pragma once

#include <algorithm>

// 计分管理类：记录命中与失误
class ScoreManager
{
public:
    // 记录命中
    void recordHit()
    {
        ++hits;
        ++totalShots;
    }

    // 记录失误
    void recordMiss()
    {
        ++misses;
        ++totalShots;
    }

    // 获取命中率
    float getAccuracy() const
    {
        if (totalShots == 0)
        {
            return 0.0f;
        }
        return static_cast<float>(hits) / static_cast<float>(totalShots) * 100.0f;
    }

    // 获取命中数量
    int getHits() const
    {
        return hits;
    }

    // 获取失误数量
    int getMisses() const
    {
        return misses;
    }

private:
    int hits = 0;
    int misses = 0;
    int totalShots = 0;
};
