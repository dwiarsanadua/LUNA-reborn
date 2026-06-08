--[[
  Quest 181: Take Back the Skies
  Level Required: 38
  NPC Start: 36, NPC Complete: 37
  Prerequisites: [180]
  Rewards: EXP=91860, Gold=8350
]]

local fsm = require('fsm_engine')

local quest_181 = fsm:new({
    id = 181,
    name = "Take Back the Skies",
    level_required = 38,
    npc_start = 36,
    npc_complete = 37,
    prerequisites = {180},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 181: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 181: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 181: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 30,
                    count = 10,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 181: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 60,
                    count = 25,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 181: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 181: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 181: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 181: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 37,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 181: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 181: Rewards given")
                self:give_rewards({
                    exp = 91860,
                    gold = 8350,
                    items = {
                        { item_id = 21000096, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_181