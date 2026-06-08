--[[
  Quest 825: Another Possibility
  Level Required: 76
  NPC Start: 92, NPC Complete: 92
  Rewards: EXP=1579838, Gold=41009
]]

local fsm = require('fsm_engine')

local quest_825 = fsm:new({
    id = 825,
    name = "Another Possibility",
    level_required = 76,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 825: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 825: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 825: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 260,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 825: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 825: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 825: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 825: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 825: Rewards given")
                self:give_rewards({
                    exp = 1579838,
                    gold = 41009,
                })
            end,
            transitions = {},
        },
    },
})

return quest_825