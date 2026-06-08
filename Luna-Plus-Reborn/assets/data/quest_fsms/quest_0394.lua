--[[
  Quest 394: [repeatable][quest] Dangerous seeds
  Level Required: 58
  NPC Start: 93, NPC Complete: 93
  Rewards: EXP=459390
]]

local fsm = require('fsm_engine')

local quest_394 = fsm:new({
    id = 394,
    name = "[repeatable][quest] Dangerous seeds",
    level_required = 58,
    npc_start = 93,
    npc_complete = 93,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 394: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 394: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 394: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 256,
                    count = 150,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 394: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 394: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 394: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 394: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 394: Rewards given")
                self:give_rewards({
                    exp = 459390,
                })
            end,
            transitions = {},
        },
    },
})

return quest_394