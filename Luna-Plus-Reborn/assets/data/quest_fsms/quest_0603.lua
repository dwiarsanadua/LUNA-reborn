--[[
  Quest 603: Tower of Evil (Intermediate)
  Level Required: 40
  NPC Start: 28, NPC Complete: 29
  Rewards: EXP=75234
]]

local fsm = require('fsm_engine')

local quest_603 = fsm:new({
    id = 603,
    name = "Tower of Evil (Intermediate)",
    level_required = 40,
    npc_start = 28,
    npc_complete = 29,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 603: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 603: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 603: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 479,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 603: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 28,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 603: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 603: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 603: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 603: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 603: Rewards given")
                self:give_rewards({
                    exp = 75234,
                })
            end,
            transitions = {},
        },
    },
})

return quest_603