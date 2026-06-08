--[[
  Quest 2021: [Daily] Ogre Butchers
  Level Required: 71
  NPC Start: 411, NPC Complete: 411
  Rewards: EXP=618540
]]

local fsm = require('fsm_engine')

local quest_2021 = fsm:new({
    id = 2021,
    name = "[Daily] Ogre Butchers",
    level_required = 71,
    npc_start = 411,
    npc_complete = 411,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2021: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2021: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2021: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 89,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2021: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2021: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2021: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 411,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2021: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2021: Rewards given")
                self:give_rewards({
                    exp = 618540,
                    items = {
                        { item_id = 30000835, count = 2 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2021