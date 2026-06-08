--[[
  Quest 47: [Voyager Job Change] Seeking Stars
  Level Required: 20
  NPC Start: 11, NPC Complete: 13
  Rewards: EXP=290, Gold=530
]]

local fsm = require('fsm_engine')

local quest_47 = fsm:new({
    id = 47,
    name = "[Voyager Job Change] Seeking Stars",
    level_required = 20,
    npc_start = 11,
    npc_complete = 13,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 47: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 47: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 47: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 11,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 47: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 47: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 47: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 13,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 47: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 47: Rewards given")
                self:give_rewards({
                    exp = 290,
                    gold = 530,
                    items = {
                        { item_id = 21000009, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_47