--[[
  Quest 884: Rough Plains
  Level Required: 93
  NPC Start: 535, NPC Complete: 535
  Rewards: EXP=9240520
]]

local fsm = require('fsm_engine')

local quest_884 = fsm:new({
    id = 884,
    name = "Rough Plains",
    level_required = 93,
    npc_start = 535,
    npc_complete = 535,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 884: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 884: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 884: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 290,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 884: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 884: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 884: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 535,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 884: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 884: Rewards given")
                self:give_rewards({
                    exp = 9240520,
                    items = {
                        { item_id = 401, count = 1 },
                        { item_id = 401, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_884