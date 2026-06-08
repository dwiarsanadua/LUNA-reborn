--[[
  Quest 339: [B Class] Rageful Cyclops Elimination
  Level Required: 59
  NPC Start: 82, NPC Complete: 82
  Prerequisites: [337]
  Rewards: EXP=619384
]]

local fsm = require('fsm_engine')

local quest_339 = fsm:new({
    id = 339,
    name = "[B Class] Rageful Cyclops Elimination",
    level_required = 59,
    npc_start = 82,
    npc_complete = 82,
    prerequisites = {337},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 339: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 339: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 339: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 95,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 339: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 339: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 339: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 82,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 339: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 339: Rewards given")
                self:give_rewards({
                    exp = 619384,
                    items = {
                        { item_id = 245, count = 1 },
                        { item_id = 245, count = 50 },
                        { item_id = 30000501, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_339