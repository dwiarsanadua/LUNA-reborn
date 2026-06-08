--[[
  Quest 270: [Warrior Job Change] Compensation (Ax)
  Level Required: 20
  NPC Start: 6, NPC Complete: 6
  Prerequisites: [54]
  Rewards: EXP=4149, Gold=550
]]

local fsm = require('fsm_engine')

local quest_270 = fsm:new({
    id = 270,
    name = "[Warrior Job Change] Compensation (Ax)",
    level_required = 20,
    npc_start = 6,
    npc_complete = 6,
    prerequisites = {54},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 270: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 270: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 270: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 270: NPC talk objective met")
                    end
                },
                {
                    trigger = "any",
                    target = "COMPLETE",
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 270: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 6,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 270: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 270: Rewards given")
                self:give_rewards({
                    exp = 4149,
                    gold = 550,
                    items = {
                        { item_id = 11000129, count = 1 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_270