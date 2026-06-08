--[[
  Quest 701: Kasya's Surveying Tools
  Level Required: 0
  NPC Start: 41, NPC Complete: 41
  Prerequisites: [700]
  Rewards: EXP=199579
]]

local fsm = require('fsm_engine')

local quest_701 = fsm:new({
    id = 701,
    name = "Kasya's Surveying Tools",
    level_required = 0,
    npc_start = 41,
    npc_complete = 41,
    prerequisites = {700},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 701: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 41,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 701: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 701: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 13,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 701: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 46,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 701: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 41,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 701: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 701: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 41,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 701: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 701: Rewards given")
                self:give_rewards({
                    exp = 199579,
                    items = {
                        { item_id = 348, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_701