--[[
  Quest 245: [Guard Job Change] Mission Report
  Level Required: 20
  NPC Start: 2, NPC Complete: 10
  Prerequisites: [52]
  Rewards: EXP=360, Gold=550
]]

local fsm = require('fsm_engine')

local quest_245 = fsm:new({
    id = 245,
    name = "[Guard Job Change] Mission Report",
    level_required = 20,
    npc_start = 2,
    npc_complete = 10,
    prerequisites = {52},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 245: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 245: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 245: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 245: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 245: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 245: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 10,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 245: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 245: Rewards given")
                self:give_rewards({
                    exp = 360,
                    gold = 550,
                    items = {
                        { item_id = 21000009, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_245