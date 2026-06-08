--[[
  Quest 60: [Ruffian Job Change] The Content of the Request
  Level Required: 1
  NPC Start: 21, NPC Complete: 21
  Prerequisites: [48]
  Rewards: EXP=10530, Gold=2190
]]

local fsm = require('fsm_engine')

local quest_60 = fsm:new({
    id = 60,
    name = "[Ruffian Job Change] The Content of the Request",
    level_required = 1,
    npc_start = 21,
    npc_complete = 21,
    prerequisites = {48},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 60: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 60: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 60: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 15,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 60: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 60: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 60: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 60: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 60: Rewards given")
                self:give_rewards({
                    exp = 10530,
                    gold = 2190,
                    items = {
                        { item_id = 21, count = 1 },
                        { item_id = 21, count = 30 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_60