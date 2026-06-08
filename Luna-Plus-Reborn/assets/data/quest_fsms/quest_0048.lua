--[[
  Quest 48: [Ruffian Job Change] A Request From The Union
  Level Required: 20
  NPC Start: 11, NPC Complete: 21
  Rewards: EXP=360, Gold=550
]]

local fsm = require('fsm_engine')

local quest_48 = fsm:new({
    id = 48,
    name = "[Ruffian Job Change] A Request From The Union",
    level_required = 20,
    npc_start = 11,
    npc_complete = 21,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 48: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 48: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 48: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 48: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 48: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 48: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 21,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 48: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 48: Rewards given")
                self:give_rewards({
                    exp = 360,
                    gold = 550,
                    items = {
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_48