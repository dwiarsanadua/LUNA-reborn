--[[
  Quest 409: [Quest] Mud creatures
  Level Required: 74
  NPC Start: 92, NPC Complete: 92
  Rewards: EXP=1618141, Gold=198084
]]

local fsm = require('fsm_engine')

local quest_409 = fsm:new({
    id = 409,
    name = "[Quest] Mud creatures",
    level_required = 74,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 409: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 409: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 409: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 259,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 409: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 409: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 409: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 409: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 409: Rewards given")
                self:give_rewards({
                    exp = 1618141,
                    gold = 198084,
                })
            end,
            transitions = {},
        },
    },
})

return quest_409