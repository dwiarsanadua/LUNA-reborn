--[[
  Quest 913: Rough Skin
  Level Required: 82
  NPC Start: 98, NPC Complete: 98
  Prerequisites: [911]
  Rewards: EXP=2484438, Gold=43707
]]

local fsm = require('fsm_engine')

local quest_913 = fsm:new({
    id = 913,
    name = "Rough Skin",
    level_required = 82,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {911},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 913: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 913: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 913: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 275,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 913: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 272,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 913: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 913: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 913: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 913: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 913: Rewards given")
                self:give_rewards({
                    exp = 2484438,
                    gold = 43707,
                })
            end,
            transitions = {},
        },
    },
})

return quest_913