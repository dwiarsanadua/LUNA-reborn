--[[
  Quest 304: [C Class] Striped Lizardman Elimination
  Level Required: 38
  NPC Start: 79, NPC Complete: 79
  Prerequisites: [303]
  Rewards: EXP=127584, Gold=3400
]]

local fsm = require('fsm_engine')

local quest_304 = fsm:new({
    id = 304,
    name = "[C Class] Striped Lizardman Elimination",
    level_required = 38,
    npc_start = 79,
    npc_complete = 79,
    prerequisites = {303},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 304: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 304: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 304: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 57,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 304: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 304: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 304: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 79,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 304: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 304: Rewards given")
                self:give_rewards({
                    exp = 127584,
                    gold = 3400,
                    items = {
                        { item_id = 209, count = 1 },
                        { item_id = 209, count = 50 },
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_304