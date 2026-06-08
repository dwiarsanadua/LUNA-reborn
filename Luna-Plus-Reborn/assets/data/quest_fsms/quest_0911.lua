--[[
  Quest 911: Rough Skin
  Level Required: 82
  NPC Start: 98, NPC Complete: 98
  Rewards: EXP=2484438
]]

local fsm = require('fsm_engine')

local quest_911 = fsm:new({
    id = 911,
    name = "Rough Skin",
    level_required = 82,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 911: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 911: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 911: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 272,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 911: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 911: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 911: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 911: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 911: Rewards given")
                self:give_rewards({
                    exp = 2484438,
                })
            end,
            transitions = {},
        },
    },
})

return quest_911