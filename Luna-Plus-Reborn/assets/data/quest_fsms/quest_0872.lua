--[[
  Quest 872: Commissioned by Adrian
  Level Required: 92
  NPC Start: 90, NPC Complete: 534
  Rewards: EXP=6827532
]]

local fsm = require('fsm_engine')

local quest_872 = fsm:new({
    id = 872,
    name = "Commissioned by Adrian",
    level_required = 92,
    npc_start = 90,
    npc_complete = 534,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 872: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 872: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 872: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 289,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 872: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 90,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 872: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 534,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 872: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 872: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 534,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 872: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 872: Rewards given")
                self:give_rewards({
                    exp = 6827532,
                    items = {
                        { item_id = 400, count = 1 },
                        { item_id = 400, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_872