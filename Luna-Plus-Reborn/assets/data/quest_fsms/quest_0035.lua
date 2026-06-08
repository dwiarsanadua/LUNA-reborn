--[[
  Quest 35: Delivery of documents
  Level Required: 41
  NPC Start: 32, NPC Complete: 37
  Prerequisites: [34]
  Rewards: EXP=1044, Gold=795
]]

local fsm = require('fsm_engine')

local quest_35 = fsm:new({
    id = 35,
    name = "Delivery of documents",
    level_required = 41,
    npc_start = 32,
    npc_complete = 37,
    prerequisites = {34},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 35: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 35: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 35: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 35: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 32,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 35: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 35: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 35: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 35: Rewards given")
                self:give_rewards({
                    exp = 1044,
                    gold = 795,
                    items = {
                        { item_id = 14, count = 1 },
                        { item_id = 14, count = 1 },
                        { item_id = 21000097, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_35