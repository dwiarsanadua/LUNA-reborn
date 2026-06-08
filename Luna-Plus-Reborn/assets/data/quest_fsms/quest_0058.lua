--[[
  Quest 58: [Voyager Job Change] Signs of the Star Seekers
  Level Required: 20
  NPC Start: 20, NPC Complete: 20
  Prerequisites: [57]
  Rewards: EXP=12330, Gold=2415
]]

local fsm = require('fsm_engine')

local quest_58 = fsm:new({
    id = 58,
    name = "[Voyager Job Change] Signs of the Star Seekers",
    level_required = 20,
    npc_start = 20,
    npc_complete = 20,
    prerequisites = {57},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 58: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 58: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 58: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 14,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 58: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 58: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 58: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 20,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 58: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 58: Rewards given")
                self:give_rewards({
                    exp = 12330,
                    gold = 2415,
                    items = {
                        { item_id = 20, count = 20 },
                        { item_id = 21000009, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_58